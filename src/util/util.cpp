#include "util.hpp"

using namespace PodUtil;

ErrQ::ErrQ(const char *str): msg(str), fatal(false) {}
ErrQ::ErrQ(std::string str): msg(str), fatal(false) {}
ErrQ::ErrQ(GError *&err): msg(std::to_string(err->code) + err->message), fatal(false) {
	g_error_free(err);
	err = nullptr;
}
ErrQ::ErrQ(const char *str, bool isFatal): msg(str), fatal(isFatal) {}
ErrQ::ErrQ(std::string str, bool isFatal): msg(str), fatal(isFatal) {}
ErrQ::ErrQ(GError *&err, bool isFatal): msg(std::to_string(err->code) + err->message), fatal(isFatal) {
	g_error_free(err);
	err = nullptr;
}
ErrQ::ErrQ(const ErrQ &err): msg(err.msg), errq(err.errq), fatal(err.fatal) {}

ErrorQueue::ErrorQueue(std::string nm): name(nm) {}
ErrorQueue::ErrorQueue(std::string nm, ErrorQueuePtr padre): name(nm), parent(padre) {}

void ErrorQueue::error(ErrQ err) {
	if (!err.errq) err.errq = getPtr();
	if (parent) parent->error(err);
	else queue.push_back(err);
}
void ErrorQueue::error(ErrQ err, bool fatal) {
	err.fatal = fatal;
	if (!err.errq) err.errq = getPtr();
	if (parent) parent->error(err);
	else queue.push_back(err);
}
void ErrorQueue::setParent(std::shared_ptr<ErrorQueue> errq) {
	parent = errq;
	if (!queue.empty()) {
		for (ErrQ &err : queue) {
			parent->error(err);
		}
	}
}

// https://stackoverflow.com/a/3809435 CC BY-SA 4.0
const std::regex Regex::url{"[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)"};
const std::regex Regex::httpUrl{"https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)"};
