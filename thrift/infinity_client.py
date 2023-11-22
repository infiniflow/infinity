#!/usr/bin/env python

#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.
#

import sys
import glob

sys.path.append('gen-py')
from tutorial import Calculator
from tutorial.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


class CommonRequert:
    pass


def main():
    # Make socket
    transport = TSocket.TSocket('localhost', 9090)

    # Buffering is critical. Raw sockets are very slow
    transport = TTransport.TBufferedTransport(transport)

    # Wrap in a protocol
    protocol = TBinaryProtocol.TBinaryProtocol(transport)

    # Create a client to use the protocol encoder
    client = Calculator.Client(protocol)

    # Connect!
    transport.open()

    client.ping()
    print('ping()')

    sum_ = client.add(1, 1)
    print('1+1=%d' % sum_)

    work = Work()

    work.op = Operation.DIVIDE
    work.num1 = 1
    work.num2 = 0

    try:
        quotient = client.calculate(1, work)
        print('Whoa? You know how to divide by zero?')
        print('FYI the answer is %d' % quotient)
    except InvalidOperation as e:
        print('InvalidOperation: %r' % e)

    work.op = Operation.SUBTRACT
    work.num1 = 15
    work.num2 = 10

    diff = client.calculate(1, work)
    print('15-10=%d' % diff)

    log = client.getStruct(1)
    print('Check log: %s' % log.value)

    req = CommonRequest()
    res = client.Connect(req)
    print(res)

    req = CreateDatabaseRequest()
    req.option = None
    req.session_id = res.session_id
    req.db_name = "mydb"

    res = client.CreateDatabase(req)
    print(res)



    # Close!
    transport.close()


from sqlglot import condition, expressions as exp


def binary_exp_to_paser_exp(binary_expr_key) -> str:
    if binary_expr_key == "eq":
        return "="
    elif binary_expr_key == "gt":
        return ">"
    elif binary_expr_key == "lt":
        return "<"
    elif binary_expr_key == "gte":
        return ">="
    elif binary_expr_key == "lte":
        return "<="
    elif binary_expr_key == "neq":
        return "!="
    elif binary_expr_key == "and":
        return "and"
    elif binary_expr_key == "or":
        return "or"
    else:
        raise Exception(f"unknown binary expression: {binary_expr_key}")


def traverse_conditions(cons) -> ParsedExpr:
    if isinstance(cons, exp.Binary):
        parsed_expr = ParsedExpr()
        function_expr = FunctionExpr()
        function_expr.function_name = binary_exp_to_paser_exp(
            cons.key)  # key is the function name cover to >, <, =, and, or, etc.

        arguments = []
        for value in cons.hashable_args:
            expr = traverse_conditions(value)
            arguments.append(expr)
        function_expr.arguments = arguments
        parsed_expr.type = function_expr

        return parsed_expr

    elif isinstance(cons, exp.Column):
        parsed_expr = ParsedExpr()
        column_expr = ColumnExpr()
        column_name = [cons.alias_or_name]
        column_expr.column_name = column_name

        parsed_expr.type = column_expr
        return parsed_expr

    elif isinstance(cons, exp.Literal):
        parsed_expr = ParsedExpr()
        constant_expr = ConstantExpr()

        if cons.is_int:
            constant_expr.literal_type = LiteralType.kInt64
            constant_expr.i64_value = int(cons.output_name)
        elif cons.is_number:
            constant_expr.literal_type = LiteralType.kDouble
            constant_expr.f64_value = float(cons.output_name)
        else:
            raise Exception(f"unknown literal type: {cons}")

        parsed_expr.type = constant_expr
        return parsed_expr

    elif isinstance(cons, exp.Paren):
        for value in cons.hashable_args:
            traverse_conditions(value)
    else:
        raise Exception(f"unknown condition: {cons}")

if __name__ == '__main__':
    try:
        main()

        print(traverse_conditions(condition("c1>1 and c2<2 or c3=3.3")))


    except Thrift.TException as tx:
        print('%s' % tx.message)
