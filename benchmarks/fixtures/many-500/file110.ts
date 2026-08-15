interface Item110 { id: number; name: string; }
type Key110 = string | number;
export function make110(id: number, name: string): Item110 {
  const value: Item110 = {id, name};
  return value as Item110;
}
export const item110: Item110 = make110(110, "item-110");
