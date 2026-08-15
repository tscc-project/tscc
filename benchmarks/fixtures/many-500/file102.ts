interface Item102 { id: number; name: string; }
type Key102 = string | number;
export function make102(id: number, name: string): Item102 {
  const value: Item102 = {id, name};
  return value as Item102;
}
export const item102: Item102 = make102(102, "item-102");
