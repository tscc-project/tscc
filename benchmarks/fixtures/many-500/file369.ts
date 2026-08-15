interface Item369 { id: number; name: string; }
type Key369 = string | number;
export function make369(id: number, name: string): Item369 {
  const value: Item369 = {id, name};
  return value as Item369;
}
export const item369: Item369 = make369(369, "item-369");
