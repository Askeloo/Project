#ifndef UNITOFWORK_H
#define UNITOFWORK_H

#include <QString>
#include <atomic>
#include "statement.h"

namespace SQLDAL {
	class UnitOfWork {
	public:
		UnitOfWork();
		~UnitOfWork();

		template <typename T>
		std::shared_ptr<T> GetEntity();

	private:
		std::shared_ptr<Connect> Connect_;
		static std::atomic<unsigned int> Connect_number_;
		QString GenerateNewConnection();
	};

	template <typename T>
	std::shared_ptr<T> UnitOfWork::GetEntity() {
		return std::make_shared<T>(Connect_);
	}
}
#endif  // !UNITOFWORK_H
