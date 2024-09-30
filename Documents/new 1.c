// Command Data Structure

// Describes a simple command and arguments
struct SimpleCommand {
	// Available space for arguments currently preallocated
	int _numberOfAvailableArguments ;

	// Number of arguments
	int _numberOfArguments;

	// Array of arguments
	char ** _arguments;

	SimpleCommand();
	void insertArgument(char *argument);
};

// Describes a complete command with the multiple pipes if any
// and input/output redirection if any
struct Command {
	int _numberOfAvailableSimpleCommands;
	int _numberOfSimpleCommands;
	SimpleCommand ** _simpleCommands;
	char * _outputFile;
	char * _inputFile;
	char * _errFile;
	int _background;

	void prompt();
	void print();
	void execute();
	void clear();

	Command();
	void insertSimpleCommand( SimpleCommand * simpleCommand );

	static Command _currentCommand;
	static SimpleCommand *_currentSimpleCommand;
};


http://www.c-jump.com/bcc/c123d/Lectures/wk08_struct/W08_0050_c_struct_constructo.htm
