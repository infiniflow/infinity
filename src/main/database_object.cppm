//
// Created by jinhai on 23-10-31.
//

export module database_object;

namespace infinity {

export enum class ObjectType {
    kInfinity,
    kDatabase,
    kTable,
    kInvalid,
};

export class DatabaseObject {
public:
    explicit DatabaseObject(ObjectType object_type) : object_type_(object_type) {}

private:
    ObjectType object_type_ { ObjectType::kInvalid };
};

}

