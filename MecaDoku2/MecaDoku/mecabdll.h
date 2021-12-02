
#ifndef MECAB_MECABDLL_H_
#define MECAB_MECABDLL_H_

#include "mecab.h"


//typedef HRESULT (CALLBACK* LPFNDLLFUNC1)(DWORD,UINT*);
typedef mecab_model_t *( CALLBACK *LPmecab_model_new )( int , char ** );
extern LPmecab_model_new fpmecab_model_new;

typedef const char *( CALLBACK *LPgetLastError )();
extern LPgetLastError fpgetLastError;

typedef mecab_t *( CALLBACK *LPmecab_model_new_tagger )( mecab_model_t * );
extern LPmecab_model_new_tagger fpmecab_model_new_tagger;

typedef const mecab_dictionary_info_t *( CALLBACK *LPmecab_dictionary_info )( mecab_t * );
extern LPmecab_dictionary_info fpmecab_dictionary_info;

typedef mecab_lattice_t *( CALLBACK *LPmecab_model_new_lattice )( mecab_model_t * );
extern LPmecab_model_new_lattice fpmecab_model_new_lattice;

typedef void ( CALLBACK *LPmecab_destroy )( mecab_t * );
extern LPmecab_destroy fpmecab_destroy;

typedef void ( CALLBACK *LPmecab_lattice_set_sentence )( mecab_lattice_t *, const char * );
extern LPmecab_lattice_set_sentence fpmecab_lattice_set_sentence;

typedef int ( CALLBACK *LPmecab_parse_lattice )( mecab_t *, mecab_lattice_t * );
extern LPmecab_parse_lattice fpmecab_parse_lattice;

typedef mecab_node_t * ( CALLBACK *LPmecab_lattice_get_bos_node )( mecab_lattice_t * );
extern LPmecab_lattice_get_bos_node fpmecab_lattice_get_bos_node;

typedef void ( CALLBACK *LPmecab_lattice_destroy )( mecab_lattice_t * );
extern LPmecab_lattice_destroy fpmecab_lattice_destroy;

typedef void ( CALLBACK *LPmecab_model_destroy )( mecab_model_t * );
extern LPmecab_model_destroy fpmecab_model_destroy;

typedef const char * ( CALLBACK *LPmecab_lattice_tostr )( mecab_lattice_t * );
extern LPmecab_lattice_tostr fpmecab_lattice_tostr;

typedef const char * ( CALLBACK *LPmecab_lattice_set_boundary_constraint )( mecab_lattice_t *, size_t, int );
extern LPmecab_lattice_set_boundary_constraint fpmecab_lattice_set_boundary_constraint;



//extern HINSTANCE hMecabDLL;						// Handle to DLL

extern mecab_model_t *mecab_model_new_build_option();
extern HINSTANCE OpenMecabDll();
extern void CloseMecabDll();
extern BOOL isOpenMecab();
extern long getmecabDllLastError();

#endif  /* MECAB_MECABDLL_H_ */
