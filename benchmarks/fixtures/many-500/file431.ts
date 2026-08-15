interface Item431 { id: number; name: string; }
type Key431 = string | number;
export function make431(id: number, name: string): Item431 {
  const value: Item431 = {id, name};
  return value as Item431;
}
export const item431: Item431 = make431(431, "item-431");
