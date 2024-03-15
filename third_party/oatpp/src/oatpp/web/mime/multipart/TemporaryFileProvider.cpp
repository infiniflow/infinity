/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "TemporaryFileProvider.hpp"

#include "oatpp/core/data/resource/TemporaryFile.hpp"

namespace oatpp { namespace web { namespace mime { namespace multipart {

TemporaryFileProvider::TemporaryFileProvider(const oatpp::String& tmpDirectory, v_int32 randomWordSizeBytes)
  : m_tmpDirectory(tmpDirectory)
  , m_randomWordSizeBytes(randomWordSizeBytes)
{}

std::shared_ptr<data::resource::Resource> TemporaryFileProvider::getResource(const std::shared_ptr<Part>& part) {
  (void)part;
  return std::make_shared<data::resource::TemporaryFile>(m_tmpDirectory, m_randomWordSizeBytes);
}

async::CoroutineStarter TemporaryFileProvider::getResourceAsync(const std::shared_ptr<Part>& part,
                                                                std::shared_ptr<data::resource::Resource>& stream)
{
  (void)part;
  stream = std::make_shared<data::resource::TemporaryFile>(m_tmpDirectory, m_randomWordSizeBytes);
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Other functions

std::shared_ptr<PartReader> createTemporaryFilePartReader(const oatpp::String& tmpDirectory,
                                                          v_int32 randomWordSizeBytes,
                                                          v_io_size maxDataSize)
{
  auto provider = std::make_shared<TemporaryFileProvider>(tmpDirectory, randomWordSizeBytes);
  auto reader = std::make_shared<StreamPartReader>(provider, maxDataSize);
  return reader;
}

std::shared_ptr<AsyncPartReader> createAsyncTemporaryFilePartReader(const oatpp::String& tmpDirectory,
                                                                    v_int32 randomWordSizeBytes,
                                                                    v_io_size maxDataSize)
{
  auto provider = std::make_shared<TemporaryFileProvider>(tmpDirectory, randomWordSizeBytes);
  auto reader = std::make_shared<AsyncStreamPartReader>(provider, maxDataSize);
  return reader;
}

}}}}
