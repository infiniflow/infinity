# Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

import time
import helper

document_passages = [
    "Alan Turing  was an English mathematician, computer scientist, logician, cryptanalyst, philosopher and theoretical biologist.",
    "Born in Maida Vale, London, Turing was raised in southern England. He graduated from King's College, Cambridge, with a degree in mathematics.",
    "After the war, Turing worked at the National Physical Laboratory, where he designed the Automatic Computing Engine, one of the first designs for a stored-program computer.",
    "Turing has an extensive legacy with statues of him and many things named after him, including an annual award for computer science innovations.",
    "This is an irrelevant sentence which will not help for the question which user may ask in the future.",
    "This is another irrelevant sentence which will not help for the question about Turing which user may ask in the future.",
    "This is another irrelevant sentence which will not help for the question about Alan Turing which user may ask in the future.",
    "This is another irrelevant sentence which will not help for the question about the person Alan Turing which user may ask in the future.",
]


class TestHelper:
    def test(self):
        self.h = helper.InfinityHelperForColBERT()
        self.h.create_test_env({"n": {"type": "int"}})
        for i, p in enumerate(document_passages):
            self.h.insert_data({'n': i}, p)
        time.sleep(3)
        self.h.show_data()
        self.h.query_bm25('Who was Alan Turing?', ['n'], 10)
        self.h.query_float('Who was Alan Turing?', ['n'], 10)
        self.h.query_bit('Who was Alan Turing?', ['n'], 10)
        self.h.clear_test_env()


if __name__ == '__main__':
    t = TestHelper()
    t.test()
