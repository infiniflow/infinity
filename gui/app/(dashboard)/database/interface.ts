import { Leaf } from './constants';

export type TreeParentId = string | number | null;

export interface PureDatabaseRouteParams {
  databaseId: string;
  tableId: string;
}

export interface DatabaseRouteParams {
  params: Promise<PureDatabaseRouteParams>;
  searchParams: Promise<{ tab: Leaf }>;
}
