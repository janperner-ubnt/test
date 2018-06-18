/**
 * @file args.hpp
 * @brief Command line argument helper.
 *
 * Preprocesses the arguments into a usable form.
 *
 * @author Karel Dolezal, akwky@centrum.cz
 */
 
#ifndef _ARGS_HPP_
#define _ARGS_HPP_
 
/**
 * @brief Holds the command line arguments and their processed content.
 */
class Arguments // @JP@ should be decorated by final; the virtual dtor definition shall be defined otherwise
{
public:
  // @JP@ I'd prefer parsing done in ctor, rather than storing ptr to argument array,
  // there is not clear ownership, when the object is being copied
  Arguments(int argc, char *argv[]);

  bool parse();
  
  std::string host() const;
  std::string command() const;
  
private:
  /// Source data
  int v_argc;
  char **v_argv;
  
  /// Processed data
  std::string v_host;
  std::string v_command;
};

#endif