'use client';

import { ITableSegment } from '@/lib/databse-interface';
import { ColumnDef } from '@tanstack/react-table';
import { useFetchTableSegments } from 'app/(dashboard)/database/hooks';
import { DataTable } from 'app/(dashboard)/database/infinity-table';
import { PureDatabaseRouteParams } from 'app/(dashboard)/database/interface';

export const columns: ColumnDef<ITableSegment>[] = [
  {
    accessorKey: 'id',
    header: 'Id'
  },
  {
    accessorKey: 'size',
    header: 'Size'
  },
  {
    accessorKey: 'status',
    header: 'Status'
  }
];

export function TableSegments({
  tableId,
  databaseId
}: PureDatabaseRouteParams) {
  const { tableSegments } = useFetchTableSegments({ tableId, databaseId });

  return (
    <div className="container mx-auto py-10">
      <DataTable columns={columns} data={tableSegments} />
    </div>
  );
}
