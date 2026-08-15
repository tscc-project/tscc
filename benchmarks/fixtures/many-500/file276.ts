interface Item276 { id: number; name: string; }
type Key276 = string | number;
export function make276(id: number, name: string): Item276 {
  const value: Item276 = {id, name};
  return value as Item276;
}
export const item276: Item276 = make276(276, "item-276");
