/* Modified GStreamer audio filter example class
 * 
 * This is a audio filter template element
 * which means it cab be used by anyone to 
 * implement a audio filter. For now, it does 
 * not do anything fancy in
 * gst_audio_filter_template_filter, or
 * gst_audio_filter_template_filter_inplace.
 * 
 * Element gets regstered as audiofiltertemplate.
 * to compile it, replace the file of the same name 
 * in  git://anongit.freedesktop.org/gstreamer/gst-template.git.
 * 
 * sudo cp /usr/local/lib/gstreamer-1.0/libgstaudiofilterexample.so 
 * /usr/lib/x86_64-linux-gnu/gstreamer-1.0/ 
 *
 * Example launch line</title>
 * gst-launch -v -m audiotestsrc ! audiofiltertemplate ! autoaudiosink
 *
 * Mathew P Joseph <mathew.p.joseph@gmail.com>
 *
 */
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <gst/audio/gstaudiofilter.h>
#include <string.h>

GST_DEBUG_CATEGORY_STATIC (audiofiltertemplate_debug);
#define GST_CAT_DEFAULT audiofiltertemplate_debug

typedef struct _GstAudioFilterTemplate GstAudioFilterTemplate;
typedef struct _GstAudioFilterTemplateClass GstAudioFilterTemplateClass;

/* These are boilerplate cast macros and type check macros */
#define GST_TYPE_AUDIO_FILTER_TEMPLATE \
  (gst_audio_filter_template_get_type())
#define GST_AUDIO_FILTER_TEMPLATE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_AUDIO_FILTER_TEMPLATE,GstAudioFilterTemplate))
#define GST_AUDIO_FILTER_TEMPLATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_AUDIO_FILTER_TEMPLATE,GstAudioFilterTemplateClass))
#define GST_IS_AUDIO_FILTER_TEMPLATE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_AUDIO_FILTER_TEMPLATE))
#define GST_IS_AUDIO_FILTER_TEMPLATE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_AUDIO_FILTER_TEMPLATE))

struct _GstAudioFilterTemplate
{
  GstAudioFilter audiofilter;

  /* here you can add additional per-instance
   * data such as properties */
};

struct _GstAudioFilterTemplateClass
{
  GstAudioFilterClass audiofilter_class;
};


enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  ARG_0
  /* FILL ME */
};
/***************************************************
* In case you want to see 
*
* Pad Templates:
*  SINK template: 'sink'
*    Availability: Always
*    Capabilities:
*      audio/x-raw
*                 format: S16LE
*                   rate: [ 1, 2147483647 ]
*               channels: [ 1, 2147483647 ]

*  SRC template: 'src'
*    Availability: Always
*    Capabilities:
*      ANY
*
*
* instead of 
*
* Pad Templates:
*  SINK template: 'sink'
*    Availability: Always
*    Capabilities:
*      audio/x-raw
*                 format: S16LE
*                   rate: [ 1, 2147483647 ]
*               channels: [ 1, 2147483647 ]
*
*  SRC template: 'src'
*    Availability: Always
*    Capabilities:
*      audio/x-raw
*                 format: S16LE
*                   rate: [ 1, 2147483647 ]
*               channels: [ 1, 2147483647 ]
*
*
* set 
*
* static GstStaticPadTemplate src_template =
*  GST_STATIC_PAD_TEMPLATE (
*   "src",
*   GST_PAD_SRC,
*   GST_PAD_ALWAYS,
*   GST_STATIC_CAPS ("ANY")
*  );
***************************************************/

G_DEFINE_TYPE (GstAudioFilterTemplate, gst_audio_filter_template,
    GST_TYPE_AUDIO_FILTER);

static void gst_audio_filter_template_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_audio_filter_template_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean gst_audio_filter_template_setup (GstAudioFilter * filter,
    const GstAudioInfo * info);
static GstFlowReturn gst_audio_filter_template_filter (GstBaseTransform * bt,
    GstBuffer * outbuf, GstBuffer * inbuf);
static GstFlowReturn
gst_audio_filter_template_filter_inplace (GstBaseTransform * base_transform,
    GstBuffer * buf);

/* For simplicity only support 16-bit pcm in native endianness for starters */
#define SUPPORTED_CAPS_STRING GST_AUDIO_CAPS_MAKE(GST_AUDIO_NE(S16))

/* GObject vmethod implementations */
static void
gst_audio_filter_template_class_init (GstAudioFilterTemplateClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *element_class;
  GstBaseTransformClass *btrans_class;

/*****************************************************************************
*
* struct GstAudioFilterClass {
*  GstBaseTransformClass basetransformclass;
*
*  virtual function, called whenever the format changes 
*  gboolean  (*setup) (GstAudioFilter * filter, GstRingBufferSpec * format);
*
* };
*
*  In addition to the setup virtual function, you should also override 
*  the GstBaseTransform::transform and/or GstBaseTransform::transform_ip 
*  virtual function. Hence,
*  btrans_class->transform = gst_audio_filter_template_filter;
*  btrans_class->transform_ip = gst_audio_filter_template_filter_inplace;
* 
******************************************************************************/
  GstAudioFilterClass *audio_filter_class;
  GstCaps *caps;

  gobject_class = (GObjectClass *) klass;
  element_class = (GstElementClass *) klass;
  btrans_class = (GstBaseTransformClass *) klass;
  audio_filter_class = (GstAudioFilterClass *) klass;

  gobject_class->set_property = gst_audio_filter_template_set_property;
  gobject_class->get_property = gst_audio_filter_template_get_property;

  /* this function will be called when the format is set before the
   * first buffer comes in, and whenever the format changes */
  audio_filter_class->setup = gst_audio_filter_template_setup;

  /* here you set up functions to process data (either in place, or from
   * one input buffer to another output buffer); only one is required */
  btrans_class->transform = gst_audio_filter_template_filter;
  btrans_class->transform_ip = gst_audio_filter_template_filter_inplace;
//  gst_base_transform_set_in_place(btrans_class, FALSE);
  gst_base_transform_set_passthrough(btrans_class, TRUE);

  /* Note : if btrans_class->transform is commeented */  
 
  /* Set some basic metadata about your new element */
  gst_element_class_set_details_simple (element_class,
    "Audio Filter", 
    "Filter/Effect/Audio",
    "Does nothing fancy", 
    "Mathew Joseph <mathew.p.joseph@gmail.com"); 

  caps = gst_caps_from_string (SUPPORTED_CAPS_STRING);
  gst_audio_filter_class_add_pad_templates (audio_filter_class, caps);

/******************************************************
* if you want to change src capabilities uncomment
*  gst_element_class_add_pad_template (element_class,
*      gst_static_pad_template_get (&src_template));
*
******************************************************/
  gst_caps_unref (caps);
}

static void
gst_audio_filter_template_init (GstAudioFilterTemplate * filter)
{
  /* This function is called when a new filter object is created. You
   * would typically do things like initialise properties to their
   * default values here if needed. */
}

static void
gst_audio_filter_template_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstAudioFilterTemplate *filter = GST_AUDIO_FILTER_TEMPLATE (object);
  /* As of now no element properties defined */
  GST_OBJECT_LOCK (filter);
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  GST_OBJECT_UNLOCK (filter);
}

static void
gst_audio_filter_template_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstAudioFilterTemplate *filter = GST_AUDIO_FILTER_TEMPLATE (object);
  /* As of now no element properties defined */
  GST_OBJECT_LOCK (filter);
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
  GST_OBJECT_UNLOCK (filter);
}

/*  virtual function, called whenever the format changes 
*   Mathew: Not sure when the format chamnges though...
*/
static gboolean
gst_audio_filter_template_setup (GstAudioFilter * filter,
    const GstAudioInfo * info)
{
  GstAudioFilterTemplate *filter_template;
  GstAudioFormat fmt;
  gint chans, rate;

  filter_template = GST_AUDIO_FILTER_TEMPLATE (filter);

  rate = GST_AUDIO_INFO_RATE (info);
  chans = GST_AUDIO_INFO_CHANNELS (info);
  fmt = GST_AUDIO_INFO_FORMAT (info);

  GST_INFO_OBJECT (filter_template, "format %d (%s), rate %d, %d channels",
      fmt, GST_AUDIO_INFO_NAME (info), rate, chans);

  /* if any setup needs to be done (like memory allocated), do it here */

  /* The audio filter base class also saves the audio info in
   * GST_AUDIO_FILTER_INFO(filter) so it's automatically available
   * later from there as well */

  return TRUE;
}

/* You may choose to implement either a copying filter or an
 * in-place filter (or both).  Implementing only one will give
 * full functionality, however, implementing both will cause
 * audiofilter to use the optimal function in every situation,
 * with a minimum of memory copies. */

static GstFlowReturn
gst_audio_filter_template_filter (GstBaseTransform * base_transform,
    GstBuffer * inbuf, GstBuffer * outbuf)
{
  GstAudioFilterTemplate *filter = GST_AUDIO_FILTER_TEMPLATE (base_transform);
  GstMapInfo map_in;
  GstMapInfo map_out;

  //GST_LOG_OBJECT (filter, "transform buffer");
  g_print("transform buffer \n");

  /* FIXME: do something interesting here.  We simply copy the input data
   * to the output buffer for now. */
/***************************************************************
* gboolean
*  gst_buffer_map (GstBuffer *buffer,
*                  GstMapInfo *info,
*                  GstMapFlags flags);
*
* This function fills info with the GstMapInfo of all merged 
* memory blocks in buffer .
*
****************************************************************/

  if (gst_buffer_map (inbuf, &map_in, GST_MAP_READ)) {
    if (gst_buffer_map (outbuf, &map_out, GST_MAP_WRITE)) {
      g_assert (map_out.size == map_in.size);
      memcpy (map_out.data, map_in.data, map_out.size);
      gst_buffer_unmap (outbuf, &map_out);
    }
    gst_buffer_unmap (inbuf, &map_in);
  }

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_audio_filter_template_filter_inplace (GstBaseTransform * base_transform,
    GstBuffer * buf)
{
  GstAudioFilterTemplate *filter = GST_AUDIO_FILTER_TEMPLATE (base_transform);
  GstFlowReturn flow = GST_FLOW_OK;
  GstMapInfo map;

  //GST_LOG_OBJECT (filter, "transform buffer in place");
  g_print("transform buffer in place \n");

  /* FIXME: do something interesting here.  Doing nothing means the input
   * buffer is simply pushed out as is without any modification */
  if (gst_buffer_map (buf, &map, GST_MAP_READWRITE)) {
#if 0
    switch (GST_AUDIO_FILTER_FORMAT (filter)) {
      case GST_AUDIO_FORMAT_S16LE:
      case GST_AUDIO_FORMAT_S16BE: {
        gint16 *samples = map.data;
        guint n_samples = map.size / sizeof (gint16);
        guint i;

        for (i = 0; i < n; ++n) {
          samples[i] = samples[i];
        }
        break;
      }
      default:
        g_warning ("Unexpected audio format %s!",
            GST_AUDIO_INFO_NAME (GST_AUDIO_FILTER_INFO(filter)));
        flow = GST_FLOW_ERROR;
        break;
    }
#endif
    gst_buffer_unmap (buf, &map);
  }

  return flow;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  /* Register debug category for filtering log messages
   * FIXME:exchange the string 'Template plugin' with your description */
  GST_DEBUG_CATEGORY_INIT (audiofiltertemplate_debug, "audiofiltertemplate", 0,
      "Audio filter template example");

  /* This is the name used in gst-launch-1.0 and gst_element_factory_make() */
  return gst_element_register (plugin, "audiofiltertemplate", GST_RANK_NONE,
      GST_TYPE_AUDIO_FILTER_TEMPLATE);
}

/* gstreamer looks for this structure to register plugins
 *
 * FIXME:exchange the string 'Template plugin' with you plugin description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    audiofiltertemplateplugin,
    "Audio filter template plugin",
    plugin_init,
    VERSION, "LGPL",
    "GStreamer",
    "http://gstreamer.freedesktop.org"
);
