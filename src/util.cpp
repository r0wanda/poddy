#include "util.hpp"

PodUtil::ErrQ(const char *str): msg(str) {}
PodUtil::ErrQ(GError *err): msg(std::to_string(err->code) + err->message) {}

PodUtil::ErrorQueue::ErrorQueue(std::string nm): name(nm) {}
PodUtil::ErrorQueue::ErrorQueue(std::string nm, ErrorQueue &padre): name(nm), parent(parent)
void PodUtil::ErrorQueue::error(ErrQ err);
void PodUtil::ErrorQueue::setParent(std::shared_ptr<ErrorQueue> errq) {
	parent = errq;
	if (!queue.empty()) {
		for (ErrQ &err : queue) {
			parent->error(err);
		}
	}
}
