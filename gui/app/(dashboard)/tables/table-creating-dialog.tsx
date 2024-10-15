'use client';

import { Button } from '@/components/ui/button';
import {
  Dialog,
  DialogContent,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogTrigger
} from '@/components/ui/dialog';
import { IDialogProps } from '@/lib/interfaces';
import React from 'react';
import { TableCreatingForm } from './table-creating-form';

export function TableCreatingDialog({
  children,
  visible,
  switchVisible,
  hideDialog
}: React.PropsWithChildren<IDialogProps<unknown>>) {
  return (
    <Dialog open={visible} onOpenChange={switchVisible}>
      <DialogTrigger asChild>{children}</DialogTrigger>
      <DialogContent className="sm:max-w-[425px]">
        <DialogHeader>
          <DialogTitle>Create Table</DialogTitle>
        </DialogHeader>
        <div className="grid gap-4 py-4">
          <TableCreatingForm hide={hideDialog}></TableCreatingForm>
        </div>
        <DialogFooter>
          <Button type="submit" form="database-creating">
            Save
          </Button>
        </DialogFooter>
      </DialogContent>
    </Dialog>
  );
}
