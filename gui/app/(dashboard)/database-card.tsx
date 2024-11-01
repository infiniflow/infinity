'use client';

import {
  AlertDialog,
  AlertDialogAction,
  AlertDialogCancel,
  AlertDialogContent,
  AlertDialogDescription,
  AlertDialogFooter,
  AlertDialogHeader,
  AlertDialogTitle,
  AlertDialogTrigger
} from '@/components/ui/alert-dialog';
import {
  Card,
  CardDescription,
  CardHeader,
  CardTitle
} from '@/components/ui/card';
import {
  DropdownMenu,
  DropdownMenuContent,
  DropdownMenuItem,
  DropdownMenuTrigger
} from '@/components/ui/dropdown-menu';
import { PropsWithChildren } from 'react';
import DeleteIcon from '/public/delete.svg';
import MoreIcon from '/public/more.svg';

interface IProps {
  deleteItem?: () => void;
}

const DeleteConfirm = ({
  children,
  deleteItem
}: PropsWithChildren & IProps) => {
  return (
    <AlertDialog>
      <AlertDialogTrigger asChild>{children}</AlertDialogTrigger>
      <AlertDialogContent>
        <AlertDialogHeader>
          <AlertDialogTitle>Are you sure you want to delete?</AlertDialogTitle>
          <AlertDialogDescription>
            This action cannot be undone.
          </AlertDialogDescription>
        </AlertDialogHeader>
        <AlertDialogFooter>
          <AlertDialogCancel>Cancel</AlertDialogCancel>
          <AlertDialogAction onClick={deleteItem}>Continue</AlertDialogAction>
        </AlertDialogFooter>
      </AlertDialogContent>
    </AlertDialog>
  );
};

export const DeleteDropdown = ({
  children,
  deleteItem
}: React.PropsWithChildren<{ deleteItem?: () => void }>) => {
  return (
    <DropdownMenu modal={false}>
      <DropdownMenuTrigger>{children}</DropdownMenuTrigger>
      <DropdownMenuContent>
        <DeleteConfirm deleteItem={deleteItem}>
          <DropdownMenuItem
            className="cursor-pointer"
            onSelect={(e) => e.preventDefault()}
          >
            <div className="flex items-center justify-between w-full">
              <span>Drop</span>
              <DeleteIcon className="h-4 w-4"></DeleteIcon>
            </div>
          </DropdownMenuItem>
        </DeleteConfirm>
      </DropdownMenuContent>
    </DropdownMenu>
  );
};

export function DatabaseCard({ data }: { data: string }) {
  return (
    <Card className="w-full max-w-sm ">
      <CardHeader className="pt-3">
        <div className="text-right">
          <DeleteDropdown deleteItem={() => {}}>
            <MoreIcon className="h-5 w-5 inline-block cursor-pointer"></MoreIcon>
          </DeleteDropdown>
        </div>
        <CardTitle className="text-1xl">{data}</CardTitle>
        <CardDescription>db description</CardDescription>
      </CardHeader>
    </Card>
  );
}
