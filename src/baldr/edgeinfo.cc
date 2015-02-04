#include "baldr/edgeinfo.h"

namespace valhalla {
namespace baldr {

EdgeInfo::EdgeInfo(char* ptr, const char* names_list, const size_t names_list_length)
  : names_list_(names_list), names_list_length_(names_list_length) {

  item_ = reinterpret_cast<PackedItem*>(ptr);
  ptr += sizeof(PackedItem);

  // Set street_name_offset_list_ pointer
  street_name_offset_list_ = reinterpret_cast<uint32_t*>(ptr);
  ptr += (name_count() * sizeof(uint32_t));

  // Set encoded_shape_ pointer
  encoded_shape_ = ptr;
  ptr += (encoded_shape_size() * sizeof(char));
}

EdgeInfo::~EdgeInfo() {
  //nothing to delete these are all shallow pointers for the moment held
  //by another object
}

const uint32_t EdgeInfo::name_count() const {
  return item_->fields.name_count;
}

const uint32_t EdgeInfo::encoded_shape_size() const {
  return item_->fields.encoded_shape_size ;
}

const uint32_t EdgeInfo::GetStreetNameOffset(uint8_t index) const {
  if(index < item_->fields.name_count)
    return street_name_offset_list_[index];
  else
    throw std::runtime_error("StreetNameOffset index was out of bounds");
}

const std::vector<std::string> EdgeInfo::GetNames() const {
  // Get each name
  std::vector<std::string> names;
  for (uint32_t i = 0; i < name_count(); i++) {
    uint32_t offset = GetStreetNameOffset(i);
    if (offset < names_list_length_) {
      names.push_back(names_list_ + offset);
    } else {
      throw std::runtime_error("GetNames: offset exceeds size of text list");
    }
  }
  return names;
}

const std::vector<PointLL>& EdgeInfo::shape() const {
  //if we haven't yet decoded the shape, do so
  if(encoded_shape_ != nullptr) {
    shape_ = midgard::decode<std::vector<PointLL> >(std::string(encoded_shape_, item_->fields.encoded_shape_size));
    encoded_shape_ = nullptr;
  }

  //hand it back
  return shape_;
}

}
}
