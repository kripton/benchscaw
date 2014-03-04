#include <string.h>
#include <jni.h>
#include <cpu-features.h>
extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/cpu.h"
#include "libavutil/opt.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include "benchscaw.h"

}
extern "C" {

int ffmpegNextFrame() {
	//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame cpu flags:", "flags");

	//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame", "av_read_frame");
	int done = av_read_frame(pFormatCtx, &packet);
	if(done>=0) {
		// Is this a packet from the video stream?
		if(packet.stream_index==videoStream) {
			// Decode video frame
			//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame", "avcodec_decode_video2");
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

			// Did we get a video frame?
			if(frameFinished) {
				// Convert the image from its native format to RGB
				sws_scale
				(
						sws_ctx,
						(uint8_t const * const *)pFrame->data,
						pFrame->linesize,
						0,
						pCodecCtx->height,
						pFrameRGB->data,
						pFrameRGB->linesize
				);

				// Save the frame to disk
				//if(++i<=10)
				;//SaveFrame(pFrameRGB, pFrameRGB->width, pFrameRGB->height, i);
			}
		}

		// Free the packet that was allocated by av_read_frame
		//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame", "av_free_packet");
		av_free_packet(&packet);
	}
	return done;
}

jint Java_ws_websca_benchscaw_MainActivity_directRender( JNIEnv* env, jobject thiz, jobject surface ) {
	//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame cpu flags:", "flags");



	//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame", "av_read_frame");
	int done = av_read_frame(pFormatCtx, &packet);
	if(done>=0) {
		// Is this a packet from the video stream?
		if(packet.stream_index==videoStream) {
			// Decode video frame
			//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame", "avcodec_decode_video2");
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

			// Did we get a video frame?
			if(frameFinished) {
				ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
				ANativeWindow_Buffer buffer;
				if (ANativeWindow_lock(window, &buffer, NULL) == 0) {
					// Convert the image from its native format to RGB
					AVPicture pict;
					pict.data[0] = (uint8_t*)buffer.bits;
					pict.linesize[0] = buffer.stride*4;
					sws_scale
					(
							sws_ctx,
							(uint8_t const * const *)pFrame->data,
							pFrame->linesize,
							0,
							pCodecCtx->height,
							pict.data,
							pict.linesize
					);

					//char str[200];
					//sprintf(str, "%i", buffer.width);
					///__android_log_write(ANDROID_LOG_DEBUG, "width", str);
					ANativeWindow_unlockAndPost(window);
				}
				ANativeWindow_release(window);
			}
		}

		// Free the packet that was allocated by av_read_frame
		//__android_log_write(ANDROID_LOG_DEBUG, "Benchscaw JNI native nextFrame", "av_free_packet");
		av_free_packet(&packet);
	}
	return done;
}

jint Java_ws_websca_benchscaw_MainActivity_surfaceTest( JNIEnv* env, jobject thiz, jobject surface )
{
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_surfaceTest", "surfaceTest()");
	if(ffmpegNextFrame()<0)
		return -1;
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_surfaceTest", "ANativeWindow_fromSurface()");
	ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
	ANativeWindow_Buffer buffer;
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_surfaceTest", "ANativeWindow_lock()");
	if (ANativeWindow_lock(window, &buffer, NULL) == 0) {
		int y=0;

		for(y=0;y<pFrameRGB->height;y++)
		{
			memcpy((uint8_t *)buffer.bits+(y*buffer.stride*4), pFrameRGB->data[0]+y*pFrameRGB->linesize[0], pFrameRGB->width*4);
		}
		//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_surfaceTest", "ANativeWindow_unlock_and_post()");
		ANativeWindow_unlockAndPost(window);
	}
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_surfaceTest", "ANativeWindow_release()");
	ANativeWindow_release(window);
	return 1;
}

JNIEXPORT jstring JNICALL Java_ws_websca_benchscaw_MainActivity_ffmpegOpen( JNIEnv* env,	jobject thiz, jobject surface, jstring path)
{
	ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_surfaceTest", "ANativeWindow_lock()");
	int w = ANativeWindow_getWidth(window);
	int h = ANativeWindow_getHeight(window);
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "ffmpegOpen()");
	int numBytes;

	char errstr[200];
	int err=0;

	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "av_register_all()");
	av_register_all();
	// Open video file
	//return avformat_version();
	//return  env->NewStringUTF(avformat_license());

	const char *nativeString = env->GetStringUTFChars(path, 0);

	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avformat_open_input");
	err = avformat_open_input(&pFormatCtx, nativeString, NULL, NULL);
	env->ReleaseStringUTFChars(path, nativeString);
	if(err!=0) {
		av_strerror(err, errstr, 200);
		return env->NewStringUTF(errstr);
	}

	// Retrieve stream information
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avformat_find_stream_info");
	if(avformat_find_stream_info(pFormatCtx, NULL)<0)
		return env->NewStringUTF("Couldn't find stream information"); // Couldn't find stream information

	// Dump information about file onto standard error
	//av_dump_format(pFormatCtx, 0, "/sdcard/test.webm", 0);

	// Find the first video stream
	videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
			videoStream=i;
			break;
		}
	if(videoStream==-1)
		return env->NewStringUTF("Didn't find a video stream");

	// Get a pointer to the codec context for the video stream
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;


	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avcodec_find_decoder");
	// Find the decoder for the video stream
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL) {
		return env->NewStringUTF("Codec not found");
	}

	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avcodec_open2");
	// Open codec
	if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
		return env->NewStringUTF("Could not open codec");

	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avcodec_alloc_frame()");
	// Allocate video frame
	pFrame=avcodec_alloc_frame();
	if(pFrame==NULL)
		return env->NewStringUTF("avcodec_alloc_frame() failed");
	// Allocate an AVFrame structure

	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avcodec_alloc_frame()");
	pFrameRGB=avcodec_alloc_frame();
	if(pFrameRGB==NULL)
		return env->NewStringUTF("avcodec_alloc_frame() failed");

	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avpicture_get_size()");
	// Determine required buffer size and allocate buffer
	numBytes=avpicture_get_size(AV_PIX_FMT_RGBA, w,h);
	buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	ANativeWindow_setBuffersGeometry( window, pCodecCtx->width, pCodecCtx->height, WINDOW_FORMAT_RGBA_8888);
	sws_ctx =
			sws_getContext
			(
					pCodecCtx->width,
					pCodecCtx->height,
					pCodecCtx->pix_fmt,
					(int)pCodecCtx->width,
					(int)pCodecCtx->height,
					AV_PIX_FMT_RGBA,
					SWS_FAST_BILINEAR,
					NULL,
					NULL,
					NULL
			);
	pFrameRGB->width=w;
	pFrameRGB->height=h;
	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avpicture_fill()");
	avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGBA, w, h);

	i=0;
	return env->NewStringUTF("OK");
}

JNIEXPORT jstring JNICALL Java_ws_websca_benchscaw_MainActivity_ffmpegClose( JNIEnv* env, jobject thiz )
{

	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "av_free()");
	// Free the RGB image
	av_free(buffer);
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "av_free()");
	av_free(pFrameRGB);

	// Free the YUV frame
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "av_free()");
	av_free(pFrame);

	// Close the codec
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "av_close()");
	avcodec_close(pCodecCtx);

	// Close the video file
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegOpen", "avformat_close_input()");
	avformat_close_input(&pFormatCtx);


	return env->NewStringUTF("FFMPEG Close OK");
}




/////////////cpudetect stuff

jstring
Java_ws_websca_benchscaw_MainActivity_stringFromJNI( JNIEnv* env,
		jobject thiz )
{
	AndroidCpuFamily acf = android_getCpuFamily();
	switch(android_getCpuFamily()) {
	case ANDROID_CPU_FAMILY_ARM:
		return env->NewStringUTF("ANDROID_CPU_FAMILY_ARM");
		break;
	case ANDROID_CPU_FAMILY_X86:
		return env->NewStringUTF("ANDROID_CPU_FAMILY_X86");
		break;
	case ANDROID_CPU_FAMILY_MIPS:
		return env->NewStringUTF("ANDROID_CPU_FAMILY_MIPS");
		break;
	}
	return env->NewStringUTF("UNKNOWN");
}

jboolean Java_ws_websca_benchscaw_MainActivity_getCpuArmNeon( JNIEnv* env,
		jobject thiz )
{
	if(android_getCpuFamily()!=ANDROID_CPU_FAMILY_ARM)
		return JNI_FALSE;
	if ((android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0)
		return JNI_TRUE;
	else
		return JNI_FALSE;
}

jboolean Java_ws_websca_benchscaw_MainActivity_getCpuArmv7( JNIEnv* env,
		jobject thiz )
{
	if(android_getCpuFamily()!=ANDROID_CPU_FAMILY_ARM)
		return JNI_FALSE;
	if ((android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_ARMv7) != 0)
		return JNI_TRUE;
	else
		return JNI_FALSE;
}

jboolean Java_ws_websca_benchscaw_MainActivity_getCpuArmVFPv3( JNIEnv* env,
		jobject thiz )
{
	if(android_getCpuFamily()!=ANDROID_CPU_FAMILY_ARM)
		return JNI_FALSE;
	if ((android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_VFPv3) != 0)
		return JNI_TRUE;
	else
		return JNI_FALSE;
}

jboolean Java_ws_websca_benchscaw_MainActivity_getCpuX86SSSE3( JNIEnv* env,
		jobject thiz )
{
	if(android_getCpuFamily()!=ANDROID_CPU_FAMILY_X86)
		return JNI_FALSE;
	if ((android_getCpuFeatures() & ANDROID_CPU_X86_FEATURE_SSSE3) != 0)
		return JNI_TRUE;
	else
		return JNI_FALSE;
}

jboolean Java_ws_websca_benchscaw_MainActivity_getCpuX86POPCNT( JNIEnv* env,
		jobject thiz )
{
	if(android_getCpuFamily()!=ANDROID_CPU_FAMILY_X86)
		return JNI_FALSE;
	if ((android_getCpuFeatures() & ANDROID_CPU_X86_FEATURE_POPCNT) != 0)
		return JNI_TRUE;
	else
		return JNI_FALSE;
}

jboolean Java_ws_websca_benchscaw_MainActivity_getCpuX86MOVBE( JNIEnv* env,
		jobject thiz )
{
	if(android_getCpuFamily()!=ANDROID_CPU_FAMILY_X86)
		return JNI_FALSE;
	if ((android_getCpuFeatures() & ANDROID_CPU_X86_FEATURE_MOVBE) != 0)
		return JNI_TRUE;
	else
		return JNI_FALSE;
}

jint Java_ws_websca_benchscaw_MainActivity_getCpuCount( JNIEnv* env,
		jobject thiz )
{
	return (jint)android_getCpuCount();
}


JNIEXPORT jstring JNICALL Java_ws_websca_benchscaw_MainActivity_ffmpegCpuFlags( JNIEnv* env,	jobject thiz, jint w, jint h, jstring path)
{


	char str[2000];
	int cpu_flags = av_get_cpu_flags();
	int i;

	sprintf(str, "");
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegCpuFlags", str);
	for (i = 0; cpu_flag_tab[i].flag; i++)
		if (cpu_flags & cpu_flag_tab[i].flag) {
			sprintf(str, "%s %s", str, cpu_flag_tab[i].name);

		}
	//__android_log_write(ANDROID_LOG_DEBUG, "Java_ws_websca_benchscaw_MainActivity_ffmpegCpuFlags", str);
	return env->NewStringUTF(str);
}
}

