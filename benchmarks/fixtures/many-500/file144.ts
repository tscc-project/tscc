interface Item144 { id: number; name: string; }
type Key144 = string | number;
export function make144(id: number, name: string): Item144 {
  const value: Item144 = {id, name};
  return value as Item144;
}
export const item144: Item144 = make144(144, "item-144");
