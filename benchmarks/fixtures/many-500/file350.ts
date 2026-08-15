interface Item350 { id: number; name: string; }
type Key350 = string | number;
export function make350(id: number, name: string): Item350 {
  const value: Item350 = {id, name};
  return value as Item350;
}
export const item350: Item350 = make350(350, "item-350");
