interface Item50 { id: number; name: string; }
type Key50 = string | number;
export function make50(id: number, name: string): Item50 {
  const value: Item50 = {id, name};
  return value as Item50;
}
export const item50: Item50 = make50(50, "item-50");
