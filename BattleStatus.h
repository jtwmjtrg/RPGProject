#pragma once
// debug
typedef struct _BattleStatus {
	int  maxHP;		// �ő�HP
	int  maxMP;		// �ő�MP
	int		HP;		// �q�b�g�|�C���g
	int		MP;		// �}�W�b�N�p���[
	int		STR;	// �ؗ�
 // int		INT;	// ��������
	int		DEX;	// ��p���A����
	int		AGI;	// �f�����A�s�����A���
	int		LUC;	// �K�^
	int DEF_PHYS;	// �����h��
	int DEF_HEAT;	// �M�h��
	int DEF_COLD;	// ��C�h��
	int DEF_ELEC;	// �d�C�h��
	int MUE;		// �g�p���E�G�l���M�[��
}BattleStatus;