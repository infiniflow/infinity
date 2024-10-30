import { Leaf } from 'app/(dashboard)/database/constants';
import {
  DatabaseRouteParams,
  PureDatabaseRouteParams
} from 'app/(dashboard)/database/interface';
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

const Empty = () => {
  return <div>empty</div>;
};

export default async function DatabasePage(props: DatabaseRouteParams) {
  const params = await props.params;

  const { tableId, databaseId } = params;

  const searchParams = await props.searchParams;

  const { tab } = searchParams;

  const DatabaseTable: React.FunctionComponent<PureDatabaseRouteParams> =
    TableMap[tab] ?? Empty;

  return (
    <div>
      <DatabaseTable tableId={tableId} databaseId={databaseId}></DatabaseTable>
    </div>
  );
}
