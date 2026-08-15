interface Item248 { id: number; name: string; }
type Key248 = string | number;
export function make248(id: number, name: string): Item248 {
  const value: Item248 = {id, name};
  return value as Item248;
}
export const item248: Item248 = make248(248, "item-248");
