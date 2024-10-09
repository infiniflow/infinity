# Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# show buffer
echo -e '\n-- show buffer'
curl --request GET \
     --url http://localhost:23820/instance/buffer \
     --header 'accept: application/json'

# show profiles
echo -e '\n\n-- show profiles'
curl --request GET \
     --url http://localhost:23820/instance/profiles \
     --header 'accept: application/json'

# show memindex
echo -e '\n\n-- show memindex'
curl --request GET \
     --url http://localhost:23820/instance/memindex \
     --header 'accept: application/json'

# show queries
echo -e '\n\n-- show queries'
curl --request GET \
     --url http://localhost:23820/instance/queries \
     --header 'accept: application/json'

