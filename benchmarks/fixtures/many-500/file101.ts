interface Item101 { id: number; name: string; }
type Key101 = string | number;
export function make101(id: number, name: string): Item101 {
  const value: Item101 = {id, name};
  return value as Item101;
}
export const item101: Item101 = make101(101, "item-101");
