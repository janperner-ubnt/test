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
class Arguments
{
public:
  Arguments(int argc, char *argv[]);

  bool parse();
  
  std::string host();
  std::string command();
  
private:
  /// Source data
  int v_argc;
  char **v_argv;
  
  /// Processed data
  std::string v_host;
  std::string v_command;
};

#endif