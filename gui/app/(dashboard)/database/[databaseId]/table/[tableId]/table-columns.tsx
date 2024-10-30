'use client';

import { ITableColumns } from '@/lib/databse-interface';
import { ColumnDef } from '@tanstack/react-table';
import { useFetchTableColumns } from 'app/(dashboard)/database/hooks';
import { DataTable } from 'app/(dashboard)/database/infinity-table';
import { PureDatabaseRouteParams } from 'app/(dashboard)/database/interface';
import { DataTableColumnHeader } from 'app/(dashboard)/database/table-column-header';

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
      return (
        <DataTableColumnHeader
          column={column}
          title="Default"
          className="text-center"
        />
      );
    }
  }
];

export function TableColumns({ tableId, databaseId }: PureDatabaseRouteParams) {
  const { tableColumns } = useFetchTableColumns({ tableId, databaseId });

  return (
    <div className="container mx-auto py-10">
      <DataTable columns={columns} data={tableColumns} />
    </div>
  );
}
