/* cmdline.h */

/* File autogenerated by gengetopt version 2.7.1  */

#ifndef _cmdline_h
#define _cmdline_h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Don't define PACKAGE and VERSION if we use automake.  */
#ifndef PACKAGE
#define PACKAGE "billard"
#endif

#ifndef VERSION
#define VERSION ""
#endif

struct gengetopt_args_info
{
  char * federation_arg;	/* federation name.  */
  char * name_arg;	/* federate name.  */
  int coordinated_flag;	/* coordinated time (default=on).  */
  int xoffset_arg;	/* X offset (X11).  */
  int yoffset_arg;	/* Y offset (X11).  */

  int help_given ;	/* Whether help was given.  */
  int version_given ;	/* Whether version was given.  */
  int federation_given ;	/* Whether federation was given.  */
  int name_given ;	/* Whether name was given.  */
  int coordinated_given ;	/* Whether coordinated was given.  */
  int xoffset_given ;	/* Whether xoffset was given.  */
  int yoffset_given ;	/* Whether yoffset was given.  */

} ;

int cmdline_parser (int argc, char * const *argv, struct gengetopt_args_info *args_info);

void cmdline_parser_print_help(void);
void cmdline_parser_print_version(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _cmdline_h */