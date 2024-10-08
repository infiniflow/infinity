import SideMenu, { MenuItem } from '@/components/ui/side-menu';
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeader,
  TableRow
} from '@/components/ui/table';
import { listDatabase, listTable } from '../actions';
import { InfinityContextMenuContent } from './context-menu';

const invoices = [
  {
    invoice: 'INV001',
    paymentStatus: 'Paid',
    totalAmount: '$250.00',
    paymentMethod: 'Credit Card'
  },
  {
    invoice: 'INV002',
    paymentStatus: 'Pending',
    totalAmount: '$150.00',
    paymentMethod: 'PayPal'
  },
  {
    invoice: 'INV003',
    paymentStatus: 'Unpaid',
    totalAmount: '$350.00',
    paymentMethod: 'Bank Transfer'
  },
  {
    invoice: 'INV004',
    paymentStatus: 'Paid',
    totalAmount: '$450.00',
    paymentMethod: 'Credit Card'
  },
  {
    invoice: 'INV005',
    paymentStatus: 'Paid',
    totalAmount: '$550.00',
    paymentMethod: 'PayPal'
  },
  {
    invoice: 'INV006',
    paymentStatus: 'Pending',
    totalAmount: '$200.00',
    paymentMethod: 'Bank Transfer'
  },
  {
    invoice: 'INV007',
    paymentStatus: 'Unpaid',
    totalAmount: '$300.00',
    paymentMethod: 'Credit Card'
  }
];

function InfinityTable() {
  return (
    <Table>
      <TableHeader>
        <TableRow>
          <TableHead className="w-[100px]">Name</TableHead>
          <TableHead>Type</TableHead>
          <TableHead>Default</TableHead>
        </TableRow>
      </TableHeader>
      <TableBody>
        {invoices.map((invoice) => (
          <TableRow key={invoice.invoice}>
            <TableCell className="font-medium">{invoice.invoice}</TableCell>
            <TableCell>{invoice.paymentStatus}</TableCell>
            <TableCell>{invoice.paymentMethod}</TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}

export default async function DatabasePage({
  searchParams
}: {
  searchParams: { q: string; offset: string };
}) {
  const search = searchParams.q ?? '';
  const offset = searchParams.offset ?? 0;

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
    <div className="flex divide-x ">
      <section className="w-40">
        <SideMenu
          items={items}
          contextMenuContent={(key: string) => (
            <InfinityContextMenuContent
              databaseName={key}
            ></InfinityContextMenuContent>
          )}
        ></SideMenu>
      </section>
      <section className="flex-1 text-center">
        <InfinityTable></InfinityTable>
      </section>
    </div>
  );
}
