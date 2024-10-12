'use client';

import { ITableIndex } from '@/lib/databse-interface';
import { ColumnDef } from '@tanstack/react-table';
import { useFetchTableIndexes } from 'app/(dashboard)/database/hooks';
import { DataTable } from 'app/(dashboard)/database/infinity-table';
import { DatabaseRouteParams } from 'app/(dashboard)/database/interface';

export const columns: ColumnDef<ITableIndex>[] = [
  {
    accessorKey: 'columns',
    header: 'Columns'
  },
  {
    accessorKey: 'index_name',
    header: 'Index Name'
  },
  {
    accessorKey: 'index_type',
    header: 'Index Type'
  }
];

export function TableIndexes({
  tableId,
  databaseId
}: DatabaseRouteParams['params']) {
  const { tableIndexes } = useFetchTableIndexes({ tableId, databaseId });

  return (
    <div className="container mx-auto py-10">
      <DataTable columns={columns} data={tableIndexes} />
    </div>
  );
}
