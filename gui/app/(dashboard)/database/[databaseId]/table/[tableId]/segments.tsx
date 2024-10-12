'use client';

import { ITableSegment } from '@/lib/databse-interface';
import { ColumnDef } from '@tanstack/react-table';
import { useFetchTableSegments } from 'app/(dashboard)/database/hooks';
import { DataTable } from 'app/(dashboard)/database/infinity-table';
import { DatabaseRouteParams } from 'app/(dashboard)/database/interface';

export const columns: ColumnDef<ITableSegment>[] = [
  {
    accessorKey: 'id',
    header: 'id'
  },
  {
    accessorKey: 'size',
    header: 'size'
  },
  {
    accessorKey: 'status',
    header: 'status'
  }
];

export function TableSegments({
  tableId,
  databaseId
}: DatabaseRouteParams['params']) {
  const { tableSegments } = useFetchTableSegments({ tableId, databaseId });

  return (
    <div className="container mx-auto py-10">
      <DataTable columns={columns} data={tableSegments} />
    </div>
  );
}
