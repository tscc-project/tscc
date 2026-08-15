interface Item78 { id: number; name: string; }
type Key78 = string | number;
export function make78(id: number, name: string): Item78 {
  const value: Item78 = {id, name};
  return value as Item78;
}
export const item78: Item78 = make78(78, "item-78");
