#ifndef _NETHOGS_LINE_H_
#define _NETHOGS_LINE_H_

#include <cstdlib>
#include <assert.h>
#include <signal.h>

#define PID_MAX 4194303

class Line
{
public:
	Line (const char * name, double n_recv_value, double n_sent_value, pid_t pid, uid_t uid, const char * n_devicename)
	{
		assert (pid >= 0);
		assert (pid <= PID_MAX);
		m_name = name;
		sent_value = n_sent_value;
		recv_value = n_recv_value;
		devicename = n_devicename;
		m_pid = pid;
		m_uid = uid;
		assert (m_pid >= 0);
	}

	void show (int row, unsigned int proglen);
	void log ();
	
	const char* getDevicename() const {return devicename;}
	const char* getName() const {return m_name;}
	double getPid() const {return m_pid;}
	double getUid() const {return m_uid;}

	double sent_value;
	double recv_value;
private:
	const char * m_name;
	const char * devicename;
	pid_t m_pid;
	uid_t m_uid;
};

#endif //_NETHOGS_LINE_H_
