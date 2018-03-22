Instructions to take gstvideoscale.c and insert it as a new pluign into gstreamer-vaapi

1) git clone  git://anongit.freedesktop.org/gstreamer/gstreamer-vaapi
2) git checkout 1.8.3
3) Copy gstmyvideoscale.h to gstreamer-vaapi/gst-libs/gst/vaapi

	Following changes have been made in gstmyvideoscale.h  

	- Default GST_TYPE_VIDEO_SCALE changed to GST_TYPE_MYVIDEO_SCALE
          #define GST_TYPE_MYVIDEO_SCALE \
	  (gst_myvideo_scale_get_type())

	- gst_video_scale_get_type() changed to gst_myvideo_scale_get_type()

4) Copy gstmyvideoscale.c to gstreamer-vaapi/gst-libs/gst/vaapi
5) Copy gstmyvideoscaleelem.h to gstreamer-vaapi/gst/vaapi/
6) vim ./gst-libs/gst/vaapi/Makefile.am 
	Add gstmyvideoscale.c  to libgstvaapi_source_c
	Add gstmyvideoscale.h to libgstvaapi_source_h
7) In gstvaapi.c include
 	
	#include "gstmyvideoscaleelem.h"

8)  In gstvaapi.c add

	gst_element_register (plugin, "myvideoscale",
		GST_RANK_PRIMARY, GST_TYPE_MYVIDEO_SCALE);

The GST_TYPE_MYVIDEO_SCALE is important else it gets registered as some other type

9)  gstmyvideoscaleelem.h contents
#ifndef _GST_MYVIDEOSCALEELEM_H_
#define _GST_MYVIDEOSCALEELEM_H_

#include <gst/vaapi/gstmyvideoscale.h>

G_BEGIN_DECLS


G_END_DECLS

#endif /* _GST_MYVIDEOSCALEELEM_H_ */


10) ./autogen.sh
   sudo make (you will see libgstvaapi_la-gstmyvideoscale.lo ingstreamer-vaapi/gst-libs/gst/vaapi )
   sudo make install  	

11) cd /usr/local/lib/gstreamer-1.0
   sudo cp libgstvaapi.so /usr/lib/x86_64-linux-gnu/gstreamer-1.0

12) gst-inspect-1.0 myvideoscale
13) gst-launch-1.0 filesrc location=./1080p_H264.mp4 ! qtdemux ! h264parse ! vaapidecode ! myvideoscale ! video/x-raw, width=400, height=400 ! vaapisink


Observations:

1) If you let 
G_DEFINE_TYPE (GstMyVideoScale, gst_myvideo_scale, GST_TYPE_VIDEO_FILTER); in 
gstmyvideoscale.c everything works fine, i.e, 
gst-inspect-1.0 myvideoscale shows

GObject
 +----GInitiallyUnowned
       +----GstObject
             +----GstElement
                   +----GstBaseTransform
                         +----GstVideoFilter
                               +----GstMyVideoScale

Pads:
  SINK: 'sink'
    Pad Template: 'sink'
  SRC: 'src'
    Pad Template: 'src'

and 
gst-launch-1.0 filesrc location=./1080p_H264.mp4 ! qtdemux ! h264parse ! vaapidecode ! myvideoscale ! video/x-raw, width=400, height=400 ! vaapisink


works perfect.
If you set,
G_DEFINE_TYPE (GstMyVideoScale, gst_myvideo_scale, GST_TYPE_ELEMENT);

gst-inspect-1.0 myvideoscale shows

GObject
 +----GInitiallyUnowned
       +----GstObject
             +----GstElement
                   +----GstMyVideoScale

Pads:
  none

and consequently becoz there are no pads..

gst-launch-1.0 filesrc location=./1080p_H264.mp4 ! qtdemux ! h264parse ! vaapidecode ! myvideoscale ! video/x-raw, width=400, height=400 ! vaapisink
libva info: VA-API version 0.39.2
libva info: va_getDriverName() returns 0
libva info: Trying to open /usr/lib/x86_64-linux-gnu/dri/i965_drv_video.so
libva info: Found init function __vaDriverInit_0_39
libva info: va_openDriver() returns 0

(gst-launch-1.0:8691): GStreamer-WARNING **: Element myvideoscale0 has an ALWAYS template sink, but no pad of the same name

(gst-launch-1.0:8691): GStreamer-WARNING **: Element myvideoscale0 has an ALWAYS template src, but no pad of the same name
WARNING: erroneous pipeline: could not link vaapidecode0 to myvideoscale0

you cannot use,
G_DEFINE_TYPE (GstMyVideoScale, gst_myvideo_scale, GST_TYPE_MYVIDEO_SCALE); as far as I understand is

if you see the defintion of
G_DEFINE_TYPE()
#define G_DEFINE_TYPE(TN, t_n, T_P)			    G_DEFINE_TYPE_EXTENDED (TN, t_n, T_P, 0, {})


the T_P is "T_P The GType of the parent type. "
and in this case GST_TYPE_MYVIDEO_SCALE cannot be athe parent class to GstMyVideoScale

the parent_class is "GstVideoFilterClass" 

struct _GstMyVideoScaleClass {
  GstVideoFilterClass parent_class;
};

