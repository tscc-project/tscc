interface Item482 { id: number; name: string; }
type Key482 = string | number;
export function make482(id: number, name: string): Item482 {
  const value: Item482 = {id, name};
  return value as Item482;
}
export const item482: Item482 = make482(482, "item-482");
