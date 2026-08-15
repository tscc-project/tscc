interface Item363 { id: number; name: string; }
type Key363 = string | number;
export function make363(id: number, name: string): Item363 {
  const value: Item363 = {id, name};
  return value as Item363;
}
export const item363: Item363 = make363(363, "item-363");
