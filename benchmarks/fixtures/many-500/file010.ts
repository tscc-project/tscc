interface Item10 { id: number; name: string; }
type Key10 = string | number;
export function make10(id: number, name: string): Item10 {
  const value: Item10 = {id, name};
  return value as Item10;
}
export const item10: Item10 = make10(10, "item-10");
