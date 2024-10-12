import { Leaf } from 'app/(dashboard)/database/constants';
import { DatabaseRouteParams } from 'app/(dashboard)/database/interface';
import React from 'react';
// import { TableColumns } from './columns';
import { TableIndexes } from './indexes';
import { TableSegments } from './segments';
import { TableColumns } from './table-columns';

const TableMap = {
  [Leaf.Columns]: TableColumns,
  [Leaf.Indexes]: TableIndexes,
  [Leaf.Segments]: TableSegments
};

export default async function DatabasePage({
  searchParams: { tab },
  params: { tableId, databaseId }
}: DatabaseRouteParams) {
  const DatabaseTable: React.FunctionComponent<DatabaseRouteParams['params']> =
    TableMap[tab];

  return (
    <div>
      <DatabaseTable tableId={tableId} databaseId={databaseId}></DatabaseTable>
    </div>
  );
}
