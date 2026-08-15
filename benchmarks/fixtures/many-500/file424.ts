interface Item424 { id: number; name: string; }
type Key424 = string | number;
export function make424(id: number, name: string): Item424 {
  const value: Item424 = {id, name};
  return value as Item424;
}
export const item424: Item424 = make424(424, "item-424");
