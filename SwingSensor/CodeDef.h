#pragma once

// 戻り値定義
#define RESULT_OK			0			// 戻り値:成功
#define RESULT_NG_INVALID	-1			// 戻り値:失敗(引数異常)
#define RESULT_NG_SYSERR	-2			// 戻り値:失敗(システムエラー)
#define RESULT_NG_SEQUENCE	-3			// 戻り値:失敗(シーケンスエラー)
#define RESULT_NG_BUSY		-4			// 戻り値:失敗(ビジー)

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