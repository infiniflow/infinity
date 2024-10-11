import { MenuItem } from '@/components/ui/side-menu';
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow
} from '@/components/ui/table';
import { listDatabase, listTable } from '../actions';
import AsyncTree from './tree';

async function InfinityTable() {
  const tables = await listTable('default_db');
  return (
    <Table>
      <TableHeader>
        <TableRow>
          <TableHead className="text-center">Name</TableHead>
        </TableRow>
      </TableHeader>
      <TableBody>
        {tables.tables.map((table: string) => (
          <TableRow key={table}>
            <TableCell className="font-medium">{table}</TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}

export default async function DatabaseLayout({
  searchParams,
  children
}: {
  searchParams: { q: string; offset: string };
  children: React.ReactNode;
}) {
  const search = searchParams?.q ?? '';
  const offset = searchParams?.offset ?? 0;

  const items: MenuItem[] = [
    {
      key: 'sub1',
      label: 'Navigation',
      children: [
        {
          key: 'g1',
          label: 'Item 1'
        },
        {
          key: 'g2',
          label: 'Item 2'
        }
      ]
    }
  ];

  const ret = await listDatabase();
  if (ret.databases.length > 1) {
    const latestDatabase = ret.databases.at(-1);
    const tables = await listTable(latestDatabase);
    console.log('🚀 ~ ret:', tables);
    items.push({
      key: latestDatabase,
      label: latestDatabase,
      children: tables.tables
    });
  }

  return (
    <div className="flex divide-x h-full">
      <section className="w-1/4">
        {/* <SideMenu
          items={items}
          contextMenuContent={(key: string) => (
            <InfinityContextMenuContent
              databaseName={key}
            ></InfinityContextMenuContent>
          )}
        ></SideMenu> */}
        <AsyncTree></AsyncTree>
      </section>
      <section className="flex-1 text-center">{children}</section>
    </div>
  );
}
