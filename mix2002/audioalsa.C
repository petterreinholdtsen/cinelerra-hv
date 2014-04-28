#include "audioalsa.h"
#include "guicast.h"
#include "mixer.h"
#include "mixertree.h"


/* mixer split types */
enum 
{
  MIXER_ELEM_FRONT, MIXER_ELEM_REAR,
  MIXER_ELEM_CENTER, MIXER_ELEM_WOOFER,
  MIXER_ELEM_END
};

static snd_mixer_selem_channel_id_t mixer_elem_chn[][2] = 
{
  { SND_MIXER_SCHN_FRONT_LEFT,   SND_MIXER_SCHN_FRONT_RIGHT },
  { SND_MIXER_SCHN_REAR_LEFT,    SND_MIXER_SCHN_REAR_RIGHT },
  { SND_MIXER_SCHN_FRONT_CENTER, SND_MIXER_SCHN_UNKNOWN },
  { SND_MIXER_SCHN_WOOFER,       SND_MIXER_SCHN_UNKNOWN },
};

AudioALSA::AudioALSA(Mixer *mixer)
 : AudioDriver(mixer)
{
	mixer_handle = 0;
	mixer_sid = 0;
	mixer_type = 0;
	mixer_grpidx = 0;
}

AudioALSA::~AudioALSA()
{
	if(mixer_handle) snd_mixer_close(mixer_handle);
	if(mixer_sid) free(mixer_sid);
	if(mixer_type) free(mixer_type);
	if(mixer_grpidx) free(mixer_grpidx);
}

static int mixer_event(snd_mixer_t *mixer, 
	unsigned int mask, 
	snd_mixer_elem_t *elem)
{
  	return 0;
}

int AudioALSA::construct_tree()
{
	int result = 0;
  	snd_ctl_card_info_t *hw_info;
  	snd_ctl_t *ctl_handle;
	snd_ctl_card_info_alloca(&hw_info);
	char card_id[BCTEXTLEN];

	strcpy(card_id, "default");
	mixer->defaults->get("ALSA_CARDID", card_id);
	if((result = snd_ctl_open(&ctl_handle, card_id, 0)) < 0)
		fprintf(stderr, "AudioALSA::construct_tree: snd_ctl_open failed\n");
	if((result = snd_ctl_card_info(ctl_handle, hw_info)) < 0)
		fprintf(stderr, "AudioALSA::construct_tree: snd_ctl_card_info failed\n");
	snd_ctl_close(ctl_handle);

	if((result = snd_mixer_open(&mixer_handle, 0)) < 0)
		fprintf(stderr, "AudioALSA::construct_tree: snd_mixer_ope failed.\n");
	if((result = snd_mixer_attach(mixer_handle, card_id)) < 0)
		fprintf(stderr, "AudioALSA::construct_tree: snd_mixer_attach failed.\n");
	if((result = snd_mixer_selem_register(mixer_handle, NULL, NULL)) < 0)
		fprintf(stderr, "AudioALSA::construct_tree: snd_mixer_selem_register failed.\n");
	snd_mixer_set_callback(mixer_handle, mixer_event);
	if((result = snd_mixer_load(mixer_handle)) < 0)
		fprintf(stderr, "AudioALSA::construct_tree: snd_mixer_load failed.\n");







  	snd_mixer_elem_t *elem;
  	snd_mixer_selem_id_t *sid;
  	snd_mixer_selem_id_t *focus_gid;
	snd_mixer_selem_id_alloca(&focus_gid);
	int idx;
	int elem_index;

	mixer_n_selems = 0;
	mixer_sid = malloc(snd_mixer_selem_id_sizeof() * 
		snd_mixer_get_count(mixer_handle));
	for(elem = snd_mixer_first_elem(mixer_handle); 
		elem; 
		elem = snd_mixer_elem_next(elem)) 
	{
		sid = (snd_mixer_selem_id_t*)(((char *)mixer_sid) + 
			snd_mixer_selem_id_sizeof() * mixer_n_selems);
		snd_mixer_selem_get_id(elem, sid);
		mixer_n_selems++;
	}

  	mixer_n_elems = 0;
	for(idx = 0; idx < mixer_n_selems; idx++) 
	{
		sid = (snd_mixer_selem_id_t*)(((char *)mixer_sid) + 
			snd_mixer_selem_id_sizeof() * idx);
		elem = snd_mixer_find_selem(mixer_handle, sid);

	    for(int i = 0; i < MIXER_ELEM_END; i++) 
		{
			result = 0;
			for(int j = 0; j < 2; j++)
			{
				if(snd_mixer_selem_has_playback_channel(elem, 
					mixer_elem_chn[i][j]))
					result++;
			}
			if(result)
				mixer_n_elems++;
		}
	}

	mixer_type = (int *)malloc(sizeof(int) * mixer_n_elems);
	mixer_grpidx = (int *)malloc(sizeof(int) * mixer_n_elems);

  	elem_index = 0;
	for(idx = 0; idx < mixer_n_selems; idx++) 
	{
		sid = (snd_mixer_selem_id_t*)(((char *)mixer_sid) + 
			snd_mixer_selem_id_sizeof() * idx);
		elem = snd_mixer_find_selem(mixer_handle, sid);
		for(int i = 0; i < MIXER_ELEM_END; i++) 
		{
			result = 0;
			for(int j = 0; j < 2; j++)
			{
				if(snd_mixer_selem_has_playback_channel(elem, 
					mixer_elem_chn[i][j]))
					result++;
			}
			if(result)
			{
				mixer_grpidx[elem_index] = idx;
				mixer_type[elem_index] = i;
				elem_index++;
				if (elem_index >= mixer_n_elems)
  				  	break;
			}
		}
	}






	



	return result;
}







