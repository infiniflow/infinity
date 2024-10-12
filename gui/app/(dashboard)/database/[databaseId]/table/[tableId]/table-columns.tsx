'use client';

import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow
} from '@/components/ui/table';
import { ITableColumns } from '@/lib/databse-interface';
import {
  ColumnDef,
  flexRender,
  getCoreRowModel,
  getPaginationRowModel,
  useReactTable
} from '@tanstack/react-table';
import { useFetchTableColumns } from 'app/(dashboard)/database/hooks';
import { DatabaseRouteParams } from 'app/(dashboard)/database/interface';
import { DataTableColumnHeader } from 'app/(dashboard)/database/table-column-header';
import { DataTablePagination } from 'app/(dashboard)/database/table-pagination';

export const columns: ColumnDef<ITableColumns>[] = [
  {
    accessorKey: 'name',
    header: 'Name'
  },
  {
    accessorKey: 'type',
    header: 'Type'
  },
  {
    accessorKey: 'default',
    // header: 'Default'
    header({ column }) {
      return <DataTableColumnHeader column={column} title="Default" />;
    }
  }
];

interface DataTableProps<TData, TValue> {
  columns: ColumnDef<TData, TValue>[];
  data: TData[];
}

function DataTable<TData, TValue>({
  columns,
  data
}: DataTableProps<TData, TValue>) {
  const table = useReactTable({
    data,
    columns,
    getCoreRowModel: getCoreRowModel(),
    getPaginationRowModel: getPaginationRowModel()
  });

  return (
    <div>
      <div className="rounded-md border">
        <Table>
          <TableHeader>
            {table.getHeaderGroups().map((headerGroup) => (
              <TableRow key={headerGroup.id}>
                {headerGroup.headers.map((header) => {
                  return (
                    <TableHead key={header.id}>
                      {header.isPlaceholder
                        ? null
                        : flexRender(
                            header.column.columnDef.header,
                            header.getContext()
                          )}
                    </TableHead>
                  );
                })}
              </TableRow>
            ))}
          </TableHeader>
          <TableBody>
            {table.getRowModel().rows?.length ? (
              table.getRowModel().rows.map((row) => (
                <TableRow
                  key={row.id}
                  data-state={row.getIsSelected() && 'selected'}
                >
                  {row.getVisibleCells().map((cell) => (
                    <TableCell key={cell.id}>
                      {flexRender(
                        cell.column.columnDef.cell,
                        cell.getContext()
                      )}
                    </TableCell>
                  ))}
                </TableRow>
              ))
            ) : (
              <TableRow>
                <TableCell
                  colSpan={columns.length}
                  className="h-24 text-center"
                >
                  No results.
                </TableCell>
              </TableRow>
            )}
          </TableBody>
        </Table>
      </div>
      <div className="mt-5">
        <DataTablePagination table={table}></DataTablePagination>
      </div>
    </div>
  );
}

export function TableColumns({
  tableId,
  databaseId
}: DatabaseRouteParams['params']) {
  const { tableColumns } = useFetchTableColumns({ tableId, databaseId });

  return (
    <div className="container mx-auto py-10">
      <DataTable columns={columns} data={tableColumns} />
    </div>
  );
}
