'use client';

import {
  ContextMenuContent,
  ContextMenuItem
} from '@/components/ui/context-menu';
import { useSeDialogState } from '@/lib/hooks';
import { TableCreatingDialog } from './table-creating-dialog';
import AddIcon from '/public/add.svg';

export function InfinityContextMenuContent({
  databaseName
}: {
  databaseName: string;
}) {
  const { showDialog, visible, hideDialog, switchVisible } = useSeDialogState();
  return (
    <>
      <ContextMenuContent>
        <ContextMenuItem>
          <div className="flex justify-between w-full" onClick={showDialog}>
            Add <AddIcon className="w-5 h-5"></AddIcon>
          </div>
        </ContextMenuItem>
      </ContextMenuContent>
      <TableCreatingDialog
        visible={visible}
        hideDialog={hideDialog}
        switchVisible={switchVisible}
      ></TableCreatingDialog>
    </>
  );
}
