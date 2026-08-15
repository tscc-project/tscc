interface Item366 { id: number; name: string; }
type Key366 = string | number;
export function make366(id: number, name: string): Item366 {
  const value: Item366 = {id, name};
  return value as Item366;
}
export const item366: Item366 = make366(366, "item-366");
