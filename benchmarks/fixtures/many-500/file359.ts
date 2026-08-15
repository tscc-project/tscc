interface Item359 { id: number; name: string; }
type Key359 = string | number;
export function make359(id: number, name: string): Item359 {
  const value: Item359 = {id, name};
  return value as Item359;
}
export const item359: Item359 = make359(359, "item-359");
