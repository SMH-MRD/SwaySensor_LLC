#pragma once
enum TaskID {
	MANAGE, PLY, COMC, COMP, PR, CLERK, ANALYST
};

enum CommandID {
	COM_E_STP, COM_MODE, COM_MANUAL, COM_JOB, COM_STATUS
};

enum ControlModeID {
	MANUAL_MODE,	SEMI_AUTO_MODE,	AUTO_MODE
};

enum AutoModeID {
	AUTO_STANDBY, AUTO_INTERRUPT,	AUTO_SUSPEND, AUTO_ACTIVE
};

enum FaultID {
	MANAGE_F0 = 1,
	MANAGE_F1,
	PLY_F0 = 1001,
	PLY_F1,
	COMC_F0 = 2001,
	COMC_F1,
	COMP_F0 = 3001, 
	PR_F0 = 4001, 
	CLERK_F0 = 5001, 
	ANALYST_F0 = 6001
};