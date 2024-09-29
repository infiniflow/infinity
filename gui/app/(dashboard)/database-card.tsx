'use client';

import { toast } from '@/components/hooks/use-toast';
import {
  Card,
  CardDescription,
  CardHeader,
  CardTitle
} from '@/components/ui/card';
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuTrigger
} from '@/components/ui/dropdown-menu';
import { DropOption } from '@/lib/constant/common';
import { DropdownMenuItem } from '@radix-ui/react-dropdown-menu';
import { useRouter } from 'next/navigation';
import { dropDatabase } from './actions';
import DeleteIcon from '/public/delete.svg';
import MoreIcon from '/public/more.svg';

export const DeleteDropdown = ({
  children,
  deleteItem
}: React.PropsWithChildren<{ deleteItem: () => void }>) => {
  return (
    <DropdownMenu>
      <DropdownMenuTrigger>{children}</DropdownMenuTrigger>
      <DropdownMenuContent>
        <DropdownMenuItem className="cursor-pointer" onClick={deleteItem}>
          <div className="flex items-center justify-between w-full">
            <span>Drop</span>
            <DeleteIcon className="h-4 w-4"></DeleteIcon>
          </div>
        </DropdownMenuItem>
      </DropdownMenuContent>
    </DropdownMenu>
  );
};

export function DatabaseCard({ data }: { data: string }) {
  const router = useRouter();
  const handleDelete = async () => {
    const ret = await dropDatabase({
      database_name: data,
      drop_option: DropOption.IgnoreIfNotExists
    });
    if (ret.error_code === 0) {
      router.refresh();
      toast({
        title: 'Drop Success',
        description: ''
      });
    }
  };

  return (
    <Card className="w-full max-w-sm ">
      <CardHeader className="pt-3">
        <div className="text-right">
          <DeleteDropdown deleteItem={handleDelete}>
            <MoreIcon className="h-5 w-5 inline-block cursor-pointer"></MoreIcon>
          </DeleteDropdown>
        </div>
        <CardTitle className="text-1xl">{data}</CardTitle>
        <CardDescription>db description</CardDescription>
      </CardHeader>
    </Card>
  );
}
