interface Item381 { id: number; name: string; }
type Key381 = string | number;
export function make381(id: number, name: string): Item381 {
  const value: Item381 = {id, name};
  return value as Item381;
}
export const item381: Item381 = make381(381, "item-381");
